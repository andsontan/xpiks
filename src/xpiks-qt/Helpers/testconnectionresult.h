#ifndef TESTCONNECTIONRESULT
#define TESTCONNECTIONRESULT

#include <QString>

namespace Helpers {
    class TestConnectionResult {
    public:
        TestConnectionResult ():
            m_Result(false)
        {}

        TestConnectionResult (bool result, const QString &url) :
            m_Url(url),
            m_Result(result)
        {}

        TestConnectionResult (const TestConnectionResult &copy) :
            m_Url(copy.m_Url),
            m_Result(copy.m_Result)
        {}

    public:
        bool getResult() const { return m_Result; }
        QString getUrl() const { return m_Url; }

    private:
        QString m_Url;
        bool m_Result;
    };
}

#endif // TESTCONNECTIONRESULT

