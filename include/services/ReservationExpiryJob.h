#pragma once
// ============================================================
// include/services/ReservationExpiryJob.h
// Background job: auto-releases stale reserved IPs.
// ============================================================

namespace ipinv::services {

class ReservationExpiryJob {
public:
    static void start(int intervalSeconds = 30, int timeoutSeconds = 300);
private:
    static void runOnce(int timeoutSeconds);
};

} // namespace ipinv::services
