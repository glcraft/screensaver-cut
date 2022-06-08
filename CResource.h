
#ifdef _WIN32
#include <string>
#include <Windows.h>
#include <Winuser.h>
class Resource {
public:
    struct Parameters {
        size_t size_bytes = 0;
        void* ptr = nullptr;
    };

private:
    HRSRC hResource = nullptr;
    HGLOBAL hMemory = nullptr;

    Parameters p;

public:
    Resource(int resource_id, const std::string &resource_class);
    auto GetResourceString() const;
    
};
#endif