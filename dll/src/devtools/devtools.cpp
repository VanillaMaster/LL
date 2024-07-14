#include "devtools.h"

#include "DevToolsClient.h"
//#include "DevToolsClient.h"

void openDevTools(cef_browser_t* browser) {

    int identifier = browser->get_identifier(browser);

	cef_window_info_t windowInfo{};
	cef_browser_settings_t settings{};

    windowInfo.x = CW_USEDEFAULT;
    windowInfo.y = CW_USEDEFAULT;
    windowInfo.width = CW_USEDEFAULT;
    windowInfo.height = CW_USEDEFAULT;
    windowInfo.ex_style = WS_EX_APPWINDOW;
    windowInfo.style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
    

    auto client = new DevToolsClient(identifier);
    client->client.base.add_ref(&client->client.base);

    auto host = browser->get_host(browser);
    //host->base.add_ref(&host->base);

	host->show_dev_tools(host, &windowInfo, (_cef_client_t*)client, &settings, nullptr);

    //client->client.base.release(&client->client.base);
    //host->base.release(&host->base);
}