#pragma once
#include <memory>

template<class T>
class BASharedObject : public std::enable_shared_from_this<T>
{
};