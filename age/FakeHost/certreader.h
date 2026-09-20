#pragma once

#include <array>
#include <vector>

// One SHA-256 thumbprint per override certificate. The launcher's
// --overrideCerts=<pem file> contains the LAN server's certificates; we pin to
// their exact bytes (via SHA-256) so only certs we shipped are trusted.
inline constexpr std::size_t kCertThumbprintLength = 32;
using CertThumbprint = std::array<BYTE, kCertThumbprintLength>;

extern std::vector<CertThumbprint> CertThumbprintList;

// Parses the launcher's --overrideCerts=<pem file> into CertThumbprintList,
// one SHA-256 hash per certificate. Returns 0 on success, non-zero when the
// certs could not be read.
int ReadCertsFile();
