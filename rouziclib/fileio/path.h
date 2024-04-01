extern const char *find_last_dirchar(const char *path, int ignore_trailing);
extern char *remove_name_from_path(char *dirpath, const char *fullpath);
extern char *remove_extension_from_path(char *truncpath, const char *fullpath);
extern const char *get_filename_from_path(const char *fullpath);
extern char *append_name_to_path(char *dest, const char *path, const char *name);
extern char *extract_file_extension(const char *path, char *ext);
extern int check_path_against_extension_list(const char *path, const char *list_ext[], size_t list_count);
extern int is_path_image_file(const char *path);
extern int is_path_video_file(const char *path);
extern char *win_get_system_folder_path(int csidl);
extern char *make_appdata_path(const char *dirname, const char *filename, const int make_subdir);
extern int chdir_utf8(const char *dirname);
