int32_t dqnqt_arg_size[DQNQT_COUNT] =
{
	0,	// DQNQT_NOTYPE
	0,	// DQNQT_BACK_TO_START
	0,	// DQNQT_BRACKET_OPEN
	0,	// DQNQT_BRACKET_CLOSE
	52,	// DQNQT_LINE_THIN_ADD
	0,	// DQNQT_POINT_ADD
	0,	// DQNQT_RECT_FULL
	0,	// DQNQT_RECT_BLACK
	0,	// DQNQT_RECT_BLACK_INV
	0,	// DQNQT_PLAIN_FILL
	0,	// DQNQT_TRIANGLE
	0,	// DQNQT_TETRAGON
	0,	// DQNQT_GAIN
	0,	// DQNQT_GAIN_PARAB
	0,	// DQNQT_LUMA_COMPRESS
	0,	// DQNQT_COL_MATRIX
	0,	// DQNQT_CLIP
	0,	// DQNQT_CLAMP
	0,	// DQNQT_CIRCLE_FULL
	0,	// DQNQT_CIRCLE_HOLLOW
	0,	// DQNQT_BLIT_BILINEAR
	0,	// DQNQT_BLIT_FLATTOP
	0,	// DQNQT_BLIT_FLATTOP_ROT
	0,	// DQNQT_BLIT_AANEAREST
	0,	// DQNQT_BLIT_AANEAREST_ROT
	0,	// DQNQT_BLIT_PHOTO
	0,	// DQNQT_TEST1
};

void dqnq_init()
{
	fb->dqnq_data = calloc(fb->dqnq_as = 131072, 1);
	rl_sem_init(&fb->dqnq_read_sem, 0);
	rl_sem_init(&fb->dqnq_end_sem, 0);
	rl_mutex_init(&fb->dqnq_mutex);

	dqnq_reset();

	// Create dqnq thread
	fb->dqnq_thread_on = 1;
	rl_thread_create(&fb->dqnq_thread, dqnq_thread, &fb);
}

void dqnq_reset()
{
	rl_mutex_lock(&fb->dqnq_mutex);
	fb->dqnq_read_pos = 0;
	fb->dqnq_write_pos = 0;
	rl_mutex_unlock(&fb->dqnq_mutex);
}

enum dqnq_type dqnq_read_type_id(volatile uint8_t *data, size_t *index)
{
	enum dqnq_type type;

	#if DQNQ_TYPE_SIZE == 4
		type = *((uint32_t *) &data[*index]);
	#else
		#error Update dqnq_read_type_id() to the value of DQNQ_TYPE_SIZE
	#endif

	*index += DQNQ_TYPE_SIZE;

	return type;
}

void dqnq_write_type_id(volatile uint8_t *entry, const enum dqnq_type type)
{
	#if DQNQ_TYPE_SIZE == 4
		print_LE32((uint8_t *) entry, type);
	#else
		#error Update dqnq_write_type_id() to the value of DQNQ_TYPE_SIZE
	#endif
}

volatile uint8_t *dqnq_new_entry(const enum dqnq_type type)
{
	volatile uint8_t *entry;
	int32_t read_pos, write_pos0, write_pos1;

	int size = dqnq_entry_size(type);

	// Calculate write position after this entry is created
	write_pos0 = fb->dqnq_write_pos;
	write_pos1 = write_pos0 + size;

	// Keep this entry contiguous by skipping the end of the circular buffer if necessary
	if (write_pos1 > fb->dqnq_as)
	{
		// Add a "back to start" command if there are more than 4 bytes left at the end of the buffer
		// otherwise the jump back is implicit
		if (fb->dqnq_as - write_pos0 >= DQNQ_TYPE_SIZE)
		{
			entry = dqnq_new_entry(DQNQT_BACK_TO_START);
			dqnq_write_type_id(entry, DQNQT_BACK_TO_START);
		}

		// Go back to start
		write_pos0 = 0;
		write_pos1 = size;
	}

	// If there's just enough room at the end of the buffer for this entry
	if (write_pos1 == fb->dqnq_as)
	{
		// Go back to start next entry
		write_pos1 = 0;
	}

	// Find the entry pointer, spin-wait for enough room if the buffer is full
	do
	{
		read_pos = rl_atomic_load_i32(&fb->dqnq_read_pos);	// gets changed by the dqnq thread

		// Check that there is enough room to write
		if (write_pos0 < read_pos && write_pos1 > read_pos)	// not enough room yet
			entry = NULL;					// will have to loop around
		else							// enough room
			entry = &fb->dqnq_data[write_pos0];		// can start writing
	}
	while (entry == NULL);

	// Start the new entry by writing its type
	dqnq_write_type_id(entry, type);

	// Increment the writing position for next entry
	rl_atomic_store_i32(&fb->dqnq_write_pos, write_pos1);

	// Unblock the dqnq thread if it's waiting
	if (rl_atomic_get_and_set(&fb->dqnq_read_wait_flag, 0))
		rl_sem_post(&fb->dqnq_read_sem);

	// Return the entry pointer after the type
	return &entry[DQNQ_TYPE_SIZE];
}

int dqnq_thread(void *unused)
{
	size_t read_pos;
	int32_t read_pos_next, write_pos;
	enum dqnq_type type;
	uint8_t *dp = (uint8_t *) fb->dqnq_data;

start:
	// Read the read and write positions
	rl_mutex_lock(&fb->dqnq_mutex);
	read_pos = fb->dqnq_read_pos;
	write_pos = fb->dqnq_write_pos;
	rl_mutex_unlock(&fb->dqnq_mutex);

	// Skip end bytes if necessary and return to the start of the buffer
	if (fb->dqnq_as - read_pos < DQNQ_TYPE_SIZE)
		read_pos = 0;

	// Wait for more data if there's none left to read
	if (read_pos == write_pos)
	{
		// If the main thread is waiting for this thread to finish go back to start to make sure we finished
		if (rl_atomic_load_i32(&fb->dqnq_end_wait_flag) == 1)
		{
			rl_atomic_store_i32(&fb->dqnq_end_wait_flag, 2);
			goto start;
		}

		// Unblock the main thread if it's waiting
		if (rl_atomic_load_i32(&fb->dqnq_end_wait_flag) == 2)
		{
			rl_atomic_store_i32(&fb->dqnq_end_wait_flag, 0);
			rl_sem_post(&fb->dqnq_end_sem);
		}

		// Wait if main thread isn't waiting at the end
		if (rl_atomic_load_i32(&fb->dqnq_end_wait_flag) == 0)
		{
			// Flag the main thread to unblock this thread when it adds new data
			rl_atomic_store_i32(&fb->dqnq_read_wait_flag, 1);

			// Wait for the main thread to unblock this thread
			rl_sem_wait(&fb->dqnq_read_sem);
		}

		goto start;
	}

	// Read and execute all the instructions in the current range
	while (fb->dqnq_thread_on)
	{
		// Read type
		type = dqnq_read_type_id(fb->dqnq_data, &read_pos);

		// Calculate next entry's position
		read_pos_next = read_pos + dqnqt_arg_size[type];
		if (fb->dqnq_as - read_pos_next < DQNQ_TYPE_SIZE)
			read_pos_next = 0;

		// Read arguments and execute the instruction
		switch (type)
		{
			case DQNQT_BACK_TO_START:
				read_pos_next = 0;
				read_pos = 0;
				break;

			case DQNQT_LINE_THIN_ADD:
				xy_t p1, p2; double radius; frgb_t colour;
				p1.x = u64_as_double(read_LE64(&dp[read_pos], &read_pos));
				p1.y = u64_as_double(read_LE64(&dp[read_pos], &read_pos));
				p2.x = u64_as_double(read_LE64(&dp[read_pos], &read_pos));
				p2.y = u64_as_double(read_LE64(&dp[read_pos], &read_pos));
				radius = u32_as_float(read_LE32(&dp[read_pos], &read_pos));
				colour.r = u32_as_float(read_LE32(&dp[read_pos], &read_pos));
				colour.g = u32_as_float(read_LE32(&dp[read_pos], &read_pos));
				colour.b = u32_as_float(read_LE32(&dp[read_pos], &read_pos));
				colour.a = u32_as_float(read_LE32(&dp[read_pos], &read_pos));

				draw_line_thin_dq(p1, p2, radius, colour, 0, 1., 0);
				break;
		}

		// Debug measure to make sure the reported sizes are correct
		if (read_pos != read_pos_next)
			fprintf_rl(stderr, "Discrepancy in dqnq_thread for type %d: %d bytes\n", type, (int) read_pos - read_pos_next);

		// Iterate to the next entry's position
		read_pos = read_pos_next;
		rl_atomic_store_i32(&fb->dqnq_read_pos, read_pos_next);

		if (read_pos == write_pos)
			goto start;
	}

	return 0;
}
