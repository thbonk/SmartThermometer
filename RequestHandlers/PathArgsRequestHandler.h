/*
Copyright 2020 Thomas Bonk <thomas@meandmymac.de>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __PATHARGSREQUESTHANDLER_H__
#define __PATHARGSREQUESTHANDLER_H__

#include <WebServer.h>

class PathArgsRequestHandler : public RequestHandler {
    public:
        PathArgsRequestHandler(const String &uri, HTTPMethod method)
        : _uri(uri)
        , _method(method)
        {
            int numParams = 0, start = 0;
            do {
                start = _uri.indexOf("{}", start);
                if (start > 0) {
                    numParams++;
                    start += 2;
                }
            } while (start > 0);
            pathArgs.resize(numParams);
        }

        bool canHandle(HTTPMethod requestMethod, String requestUri) override  {
            if (_method != HTTP_ANY && (_method & requestMethod) != requestMethod)
                return false;

            if (_uri == requestUri)
                return true;

            size_t uriLength = _uri.length();
            unsigned int pathArgIndex = 0;
            unsigned int requestUriIndex = 0;
            for (unsigned int i = 0; i < uriLength; i++, requestUriIndex++) {
                char uriChar = _uri[i];
                char requestUriChar = requestUri[requestUriIndex];

                if (uriChar == requestUriChar)
                    continue;
                if (uriChar != '{')
                    return false;

                i += 2; // index of char after '}'
                if (i >= uriLength) {
                    // there is no char after '}'
                    pathArgs[pathArgIndex] = requestUri.substring(requestUriIndex);
                    return pathArgs[pathArgIndex].indexOf("/") == -1; // path argument may not contain a '/'
                }
                else
                {
                    char charEnd = _uri[i];
                    int uriIndex = requestUri.indexOf(charEnd, requestUriIndex);
                    if (uriIndex < 0)
                        return false;
                    pathArgs[pathArgIndex] = requestUri.substring(requestUriIndex, uriIndex);
                    requestUriIndex = (unsigned int) uriIndex;
                }
                pathArgIndex++;
            }

            return requestUriIndex >= requestUri.length();
        }

        bool canUpload(String requestUri) override  {
            return canHandle(HTTP_POST, requestUri);
        }

    protected:
        String _uri;
        HTTPMethod _method;
};

#endif
