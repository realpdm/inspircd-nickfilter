# inspircd-nickfilter
Inspircd module to force nicks to reflect the real user identity useful in a corporate environment

cp m_nickfilter.cpp to inspircd/src/modules
./configure --prefix=/usr/local/inspircd --enable-openssl
make
make install
