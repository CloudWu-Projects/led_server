#include <stdio.h>
#include "LedDll.h"

#define TRACE printf
int LedServerCallback( int Msg, int wParam, void* lParam)
{
    printf("LedServerCallback msg:%d \n",Msg);
	switch(Msg)
	{
		case LV_MSG_CARD_ONLINE:
			{
				LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;
				TRACE("LV_MSG_CARD_ONLINE\n");
				TRACE("pCardInfo->port:%d\n",pCardInfo->port);
				TRACE("pCardInfo->ipStr:%s\n",pCardInfo->ipStr);
				TRACE("pCardInfo->networkIdStr:%s\n",pCardInfo->networkIdStr);

				//((CDemoDlg *)AfxGetApp()->m_pMainWnd)->ComboboxAddString(TRUE,pCardInfo->networkIdStr);
			}
			break;
		case LV_MSG_CARD_OFFLINE:
			{
				LP_CARD_INFO pCardInfo = (LP_CARD_INFO)lParam;
				TRACE("LV_MSG_CARD_OFFLINE\n");
				TRACE("pCardInfo->port:%d\n",pCardInfo->port);
				TRACE("pCardInfo->ipStr:%s\n",pCardInfo->ipStr);
				TRACE("pCardInfo->networkIdStr:%s\n",pCardInfo->networkIdStr);

				//((CDemoDlg *)AfxGetApp()->m_pMainWnd)->ComboboxAddString(FALSE,pCardInfo->networkIdStr);
			}
			break;
	}
	return 0;
}

int main()
{
    printf("aaaaaaaaaaaaaa");
    CLedDll ledDll;
    ledDll.InitDll();

	ledDll.LV_RegisterLedServerCallback((SERVERINFOCALLBACK)LedServerCallback);
    int ret = ledDll.LV_LedInitServer(10008);

    printf("LV_LedInitServer ret=%d\n",ret);
    getchar();
    return 0;
}