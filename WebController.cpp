#include "espresso.h"

void webController() {

	server->on("/", HTTP_GET, []() {
		String helloWorldHTML = 
			"<!DOCTYPE html []><html> <head> <meta charset=\"UTF-8\"/> <title>API Doc</title> <style type=\"text/css\"> td, h1, h2, h3, h4, h5, p, ul, ol, li{page-break-inside: avoid;}body{font-size: 12px; font-family: Helvetica, Arial;}h1, h2{border-bottom: 1px solid #DDDDDD;}pre, code{font-family: Consolas, \"Liberation Mono\", Courier; background-color: #F4F4F4; border: 1px solid #EAEAEA; border-radius: 2px; padding: 5px;}pre{overflow: auto; overflow-y: hidden; margin-top: -8px;}pre code{border: none;}</style> </head> <body>"
			"<h1>Hi there!</h1> <h5>Here are some REST API methods. Note: There is no authentication or ssl security in this API and all requests are sent in plain text so its really only ok to use inside a local network.</h5>"
			"<h2>/gpio</h2>"
			"<p>Set or read any digital gpio PIN (0-16)</p><pre><code>GET /gpio?gpio[N]=[high|low|read]</code></pre>"
			"<p>optionally specify timeout for pin state (ex. gpio0 set low, time=1, gpio0 is switched to high after 1 second)</p><pre><code>GET /gpio?gpio[N]=[h|l|r]&time[N]=[seconds]</code></pre>"
			"<h2>/system</h2>"
			"<p>Reboot the esp8266</p><pre><code>GET /system?reboot=true</code></pre>"
			"<p>Reset/Clear the settings</p><pre><code>GET /system?reset=true</code></pre>"
			"</body></html>";

		server->send(200, "text/html", helloWorldHTML);
	});

	server->onNotFound([]() {
		server->send(404);
	});
}
