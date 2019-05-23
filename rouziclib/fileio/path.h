extern char *find_last_dirchar(char *path, int ignore_trailing);
extern char *remove_name_from_path(char *dirpath, char *fullpath);
extern char *remove_extension_from_path(char *dirpath, char *fullpath);
extern char *get_filename_from_path(char *fullpath);
extern char *append_name_to_path(char *dest, char *path, char *name);
extern char *extract_file_extension(const char *path, char *ext);
extern char *make_appdata_path(const char *dirname, const char *filename, const int make_subdir);
extern int check_path_against_extension_list(char *path, const char *list_ext[], size_t list_count);
extern int is_path_image_file(char *path);
extern int is_path_video_file(char *path);