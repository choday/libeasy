#pragma once
#include "../ebase/string_view.hpp"

namespace ehttp
{

    class http_request
    {
    public:

        ebase::string_view      method;
        ebase::string_view      path;


        ebase::string_view      session;
    };

};