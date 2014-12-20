#ifndef FTPCREDENTIALS
#define FTPCREDENTIALS

#include <QString>

namespace Models {
    class FtpCredentials {
    public:
        FtpCredentials() {}

    public:
        const QString &getHost() const { return m_Host; }
        const QString &getUsername() const { return m_Username; }
        const QString &getPassword() const { return m_Password; }

    public:
        void setHost(const QString &value) { m_Host = value; }
        void setUsername(const QString &value) { m_Username = value; }
        void setPassword(const QString &value) { m_Password = value; }

    private:
        QString m_Host;
        QString m_Username;
        QString m_Password;
    };
}

#endif // FTPCREDENTIALS

