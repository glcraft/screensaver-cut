
#ifdef _WIN32
#include "CResource.h"
Resource::Resource(int resource_id, const std::string &resource_class) {
    hResource = FindResource(nullptr, MAKEINTRESOURCEA(resource_id), resource_class.c_str());
    hMemory = LoadResource(nullptr, hResource);

    p.size_bytes = SizeofResource(nullptr, hResource);
    p.ptr = LockResource(hMemory);
}
auto Resource::GetResourceString() const {
    std::string_view dst;
    if (p.ptr != nullptr)
        dst = std::string_view(reinterpret_cast<char*>(p.ptr), p.size_bytes);
    return dst;
}
#endif