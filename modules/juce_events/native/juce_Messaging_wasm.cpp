#include <emscripten/emscripten.h>

namespace juce
{
namespace
{
void deliverWasmMessage (void* userData)
{
    std::unique_ptr<MessageManager::MessageBase::Ptr> message (
        static_cast<MessageManager::MessageBase::Ptr*> (userData));

    if (message != nullptr && *message != nullptr)
        (*message)->messageCallback();
}
} // namespace

void MessageManager::doPlatformSpecificInitialisation() {}
void MessageManager::doPlatformSpecificShutdown() {}

bool MessageManager::postMessageToSystemQueue (MessageBase* message)
{
    emscripten_async_call (deliverWasmMessage, new MessageBase::Ptr (message), 0);
    return true;
}

void MessageManager::broadcastMessage (const String&) {}

namespace detail
{
bool dispatchNextMessageOnSystemQueue (bool)
{
    return false;
}
} // namespace detail
} // namespace juce
