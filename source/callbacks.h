void StartStopConnection(GtkWidget *widget, gpointer user_data) ; 
void isServer(GtkToggleButton *widget, gpointer user_data) ; 
void setGreenStatus(gchar* s);
void setRedStatus(gchar* s);

void abortConnection();
void getPublicIp();
void aboutDialog();
void installRemove();
void showEditBookmarkWindow();
void showNewBookmarkWindow();
void hideEditBookmarkWindow();
void showBookmarks(GtkMenuItem *menuitem, gpointer user_data);
void addToBookmarks(GtkMenuItem *menuitem, gpointer user_data);
void onTreeviewBookmarksSignleButtonPressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata);
void saveBookmark();
void delBookmark();
void hideBookmarks();
void onRowActivatedBookmark (GtkTreeView *view, GtkTreePath *path, GtkTreeViewColumn  *col, gpointer user_data);
void updateBookmarks(GtkWindow *window,  gpointer   user_data);
void initBookmarksWindow();
gboolean checkClientConnectionStatus();
gboolean checkServerConnectionStatus();
void deleteLogs();
#ifdef _WIN32 
void StartProcess(char* szExe); 
#endif
void checkDependencies();
