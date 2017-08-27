#ifndef UPLOADCONTEXT
#define UPLOADCONTEXT

#include <QString>
#include <Common/defines.h>

namespace Models {
    class ProxySettings;
}

namespace libxpks {
    namespace net {
        class UploadContext {
        public:
            ~UploadContext() {
                LOG_DEBUG << "destructor for host" << m_Host;
            }

        public:
            QString m_Host;
            QString m_Username;
            QString m_Password;
            QString m_DirForVectors;
            QString m_DirForImages;
            bool m_UsePassiveMode;
            bool m_UseEPSV;
            int m_RetriesCount;
            int m_TimeoutSeconds;
            bool m_UseProxy;
            bool m_VerboseLogging;
            Models::ProxySettings *m_ProxySettings;
        };
    }
}

#endif // UPLOADCONTEXT

