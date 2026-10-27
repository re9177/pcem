#ifdef _WIN32
#define BITMAP WINDOWS_BITMAP
#include <windows.h>
#include <windowsx.h>
#undef BITMAP
#endif

#ifdef USE_PCAP_NETWORKING
#include <pcap.h>
#endif

#include "wx-utils.h"
#include "wx-sdl2.h"
#include "wx-hostconfig.h"

#include "ibm.h"
#include "config.h" // REMOVE
#include "networking/nethandler.h"
#include "networking/netbackend.h"

int hostconfig_dialog_proc(void *hdlg, int message, INT_PARAM wParam, LONG_PARAM lParam) {
        void *h;

        switch (message) {
        case WX_INITDIALOG:
                h = wx_getdlgitem(hdlg, WX_ID("IDC_COMBO_NETWORK_TYPE"));

                // add values for all known backends, so we do not need to.
                for(int i = 0; i < network_backend_get_count(); ++i) {
                        network_backend_t* backend = network_backend_get(i);
                        wx_sendmessage(h, WX_CB_ADDSTRING, 0, (LONG_PARAM) backend->name);
                }

                int current_type = config_get_int(CFG_MACHINE, "network", "type", 0);

                // reset to null driver
                if(current_type > network_backend_get_count())
                        current_type = 0;

                wx_sendmessage(h, WX_CB_SETCURSEL, 0, current_type);

                h = wx_getdlgitem(hdlg, WX_ID("IDC_COMBO_NETWORK_DEVICE"));
                wx_sendmessage(h, WX_CB_SETCURSEL, 0, 0);
                wx_enablewindow(h, FALSE);

                return TRUE;

        case WX_COMMAND:
                if (wParam == wxID_OK) {
                        int type;

                        h = wx_getdlgitem(hdlg, WX_ID("IDC_COMBO_NETWORK_TYPE"));
                        type = wx_sendmessage(h, WX_CB_GETCURSEL, 0, 0);

                        network_backend_t* backend = network_backend_get(type);

                        config_set_int(CFG_MACHINE, "network", "type", type);

                        // Save bound adapter
                        if (backend->requiresAdapterBinding) {
                                char name[64];
                                sprintf(&name[0], "%s_bindadapter", backend->id);
                        }


                        saveconfig(NULL);

                        wx_enddialog(hdlg, 1);

                        return TRUE;
                } else if (wParam == wxID_CANCEL) {
                        wx_enddialog(hdlg, 0);
                        return TRUE;
                } else if (ID_IS("IDC_COMBO_NETWORK_TYPE")) {
                        int type;

                        h = wx_getdlgitem(hdlg, WX_ID("IDC_COMBO_NETWORK_TYPE"));
                        type = wx_sendmessage(h, WX_CB_GETCURSEL, 0, 0);
                        h = wx_getdlgitem(hdlg, WX_ID("IDC_COMBO_NETWORK_DEVICE"));

                        network_backend_t* backend = network_backend_get(type);
                        if (backend->requiresAdapterBinding) {
                                // Clear the list of adapters selected
                                wx_enablewindow(h, TRUE);
                        } else {
                                wx_enablewindow(h, FALSE);
                        }

                        return TRUE;
                }
                break;
        }

        return TRUE;
}

int hostconfig_open(void *hwnd) { return wx_dialogbox(hwnd, "NetConfig", hostconfig_dialog_proc); }
