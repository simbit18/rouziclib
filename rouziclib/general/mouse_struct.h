typedef struct
{
	ctrl_id_t hover, hover_new, current;
	int hover_box_matched, hover_ided;

	int stack_recap;	// flag is on when going through the control stack after mouse events are processed
	ctrl_id_t *stack;
	size_t stack_count, stack_as;
} mouse_ctrl_id_t;

typedef struct
{
	int8_t quick;	// flag when a click is released as soon as it's pressed
	int8_t block;	// flag when a click is to be ignored after window focus regain
} mousebut_flags_t;

typedef struct
{
	int lmb, mmb, rmb, wheel, clicks;
	mousebut_flags_t lmf, mmf, rmf;
	xy_t orig;		// coordinates of the original click
} mousebut_t;

enum
{
	mouse_mod_ctrl,
	mouse_mod_alt,
	mouse_mod_shift,
	mouse_mod_gui,
	mouse_mod_count
};

typedef struct
{
	xy_t a, u, d, u_stored, prev_u;
	int8_t window_focus_flag, mouse_focus_flag, window_minimised_flag, zoom_flag, zoom_scroll_freeze, showcursor, warp, warp_prev, warp_if_move, zoom_allowed, discard_warp_first_move;
	int8_t mod_key[mouse_mod_count];
	mousebut_t b;
	mouse_ctrl_id_t *ctrl_id;
	int8_t key_state[RL_NUM_SCANCODES], key_quick[RL_NUM_SCANCODES];	// keyboard state array, -2 newly up, -1 up, 0 = initial, 1 = down, 2 = newly down, 3 = repeated down event
	#ifdef MOUSE_LOG
	int frame_index;
	buffer_t log;
	#endif
} mouse_t;
