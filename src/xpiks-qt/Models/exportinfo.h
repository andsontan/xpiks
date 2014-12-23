#ifndef EXPORTINFO_H
#define EXPORTINFO_H

namespace Models {
    class ExportInfo {
    public:
        ExportInfo() :
            m_MustSaveOriginal(false) {}
        ~ExportInfo() {}

    public:
        bool getMustSaveOriginal() const { return m_MustSaveOriginal; }
        void setMustSaveOriginal(bool value) { m_MustSaveOriginal = value; }

    private:
        bool m_MustSaveOriginal;
    };
}

#endif // EXPORTINFO_H
