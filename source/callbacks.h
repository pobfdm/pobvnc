void StartStopConnection(GtkWidget *widget, gpointer user_data) ; 
void isServer(GtkToggleButton *widget, gpointer user_data) ; 
void abortConnection();
gchar* getPublicIp();
void aboutDialog();
void installRemove();
gboolean checkClientConnectionStatus();
gboolean checkServerConnectionStatus();
void deleteLogs();
#ifdef _WIN32 
void StartProcess(char* szExe); 
#endif
