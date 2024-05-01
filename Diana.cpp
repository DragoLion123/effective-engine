#include <iostream>
#include <objbase.h>

interface IX : IUnknown {
    virtual void MethodX() = 0;
};

interface IY : IUnknown {
    virtual void MethodY() = 0;
};

interface IZ : IUnknown {
    virtual void MethodZ() = 0;
};

class CA : public IX, public IY {
public:
    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) {
        if (riid == IID_IUnknown) {
            *ppvObject = static_cast<IUnknown*>(this);
        }
        else if (riid == IID_IX) {
            *ppvObject = static_cast<IX*>(this);
        }
        else if (riid == IID_IY) {
            *ppvObject = static_cast<IY*>(this);
        }
        else {
            *ppvObject = NULL;
            return E_NOINTERFACE;
        }

        static_cast<IUnknown*>(*ppvObject)->AddRef();
        return S_OK;
    }

    STDMETHOD_(ULONG, AddRef)() {
        return ++m_RefCount;
    }

    STDMETHOD_(ULONG, Release)() {
        if (--m_RefCount == 0) {
            delete this;
            return 0;
        }
        return m_RefCount;
    }

    void MethodX() override {
        std::cout << "MethodX called." << std::endl;
    }

    void MethodY() override {
        std::cout << "MethodY called." << std::endl;
    }

private:
    ULONG m_RefCount = 1;
};

IUnknown* CreateInstance() {
    CA* pCA = new CA();
    return static_cast<IUnknown*>(pCA);
}

int main() {
    CoInitialize(NULL);

    IUnknown* pIUnknown = CreateInstance();
    if (pIUnknown == nullptr) {
        std::cerr << "Failed to create instance." << std::endl;
        CoUninitialize();
        return 1;
    }

    IX* pIX = NULL;
    if (SUCCEEDED(pIUnknown->QueryInterface(IID_IX, reinterpret_cast<void**>(&pIX)))) {
        pIX->MethodX();
        pIX->Release();
    }

    IY* pIY = NULL;
    if (SUCCEEDED(pIUnknown->QueryInterface(IID_IY, reinterpret_cast<void**>(&pIY)))) {
        pIY->MethodY();
        pIY->Release();
    }

    IZ* pIZ = NULL;
    if (FAILED(pIUnknown->QueryInterface(IID_IZ, reinterpret_cast<void**>(&pIZ)))) {
        std::cout << "Component does not support IZ interface." << std::endl;
    }

    pIUnknown->Release();
    CoUninitialize();

    return 0;
}
