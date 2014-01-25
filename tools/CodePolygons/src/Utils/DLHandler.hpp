#ifndef ABETARE__DL_HANDLER_HPP_
#define ABETARE__DL_HANDLER_HPP_

namespace Abetare
{
    namespace DLHandler
    {
	void* GetSymbol(void *handle, const char * const name);
	void* GetSymbol(const char * const name);
    }
}

#endif
