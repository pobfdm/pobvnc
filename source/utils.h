void info_message(GtkWidget *main_window,gchar* title, gchar *msg, gchar* winlabel);
gint info_YesNo(GtkWidget *main_window,gchar* title, gchar *msg, gchar* winlabel);
void err_message(GtkWidget *main_window,gchar* title, gchar *msg, gchar* winlabel);
int getBitsCpu();
char* GetKey(const gchar* File,const gchar* group ,const gchar* key);
void* SetKey(const gchar* File,const gchar* group , const gchar* key, const gchar* content);

void initGettex();
gchar* getTerm();
void copyFromResource(gchar* res, gchar* dest);
