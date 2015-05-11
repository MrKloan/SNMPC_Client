#ifndef UTILITIES_H_INCLUDED
#define UTILITIES_H_INCLUDED

gboolean regex_verification(const gchar *, gushort);
gboolean authorize_ip(Devices *, const gchar *);
void warning_dialog(GtkWidget *, gchar *);
void hide_window(GtkWidget *, GtkWidget *);
char **explode(char *, const char *, unsigned short *);
gushort *char_to_bin(const char *, unsigned int *);
void load_matrix(Project *);

char *base64_encode(const unsigned char *, size_t, size_t *);
char *base64_decode(const unsigned char *, size_t, size_t *);
char *build_decoding_table();

#endif // UTILITIES_H_INCLUDED
