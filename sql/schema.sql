-- ============================================================
-- IP Inventory Database Schema
-- PostgreSQL 14+
-- ============================================================

CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Enum for IP type
CREATE TYPE ip_type_enum AS ENUM ('IPv4', 'IPv6');

-- Enum for IP status
CREATE TYPE ip_status_enum AS ENUM ('free', 'reserved', 'assigned');

-- ============================================================
-- ip_pool: Master table of all IP addresses in inventory
-- ============================================================
CREATE TABLE IF NOT EXISTS ip_pool (
    id              UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    ip_address      VARCHAR(45) NOT NULL UNIQUE,
    ip_type         ip_type_enum NOT NULL,
    status          ip_status_enum NOT NULL DEFAULT 'free',
    service_id      VARCHAR(128),
    reserved_at     TIMESTAMPTZ,
    assigned_at     TIMESTAMPTZ,
    created_at      TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at      TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

-- Index for fast lookups by status, service_id
CREATE INDEX IF NOT EXISTS idx_ip_pool_status    ON ip_pool(status);
CREATE INDEX IF NOT EXISTS idx_ip_pool_service   ON ip_pool(service_id);
CREATE INDEX IF NOT EXISTS idx_ip_pool_ip        ON ip_pool(ip_address);
CREATE INDEX IF NOT EXISTS idx_ip_pool_type      ON ip_pool(ip_type);
CREATE INDEX IF NOT EXISTS idx_ip_pool_reserved  ON ip_pool(reserved_at) WHERE status = 'reserved';

-- ============================================================
-- ip_audit_log: Immutable audit trail for all IP events
-- ============================================================
CREATE TABLE IF NOT EXISTS ip_audit_log (
    id          BIGSERIAL PRIMARY KEY,
    ip_address  VARCHAR(45) NOT NULL,
    action      VARCHAR(64) NOT NULL,   -- 'added','reserved','assigned','terminated','released','service_changed'
    service_id  VARCHAR(128),
    old_service_id VARCHAR(128),
    performed_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    details     JSONB
);

CREATE INDEX IF NOT EXISTS idx_audit_ip       ON ip_audit_log(ip_address);
CREATE INDEX IF NOT EXISTS idx_audit_service  ON ip_audit_log(service_id);
CREATE INDEX IF NOT EXISTS idx_audit_time     ON ip_audit_log(performed_at);

-- ============================================================
-- Function: auto-update updated_at timestamp
-- ============================================================
CREATE OR REPLACE FUNCTION set_updated_at()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trg_ip_pool_updated_at
    BEFORE UPDATE ON ip_pool
    FOR EACH ROW EXECUTE FUNCTION set_updated_at();

-- ============================================================
-- View: Summary of IP pool by status and type
-- ============================================================
CREATE OR REPLACE VIEW v_ip_pool_summary AS
SELECT
    ip_type,
    status,
    COUNT(*) AS count
FROM ip_pool
GROUP BY ip_type, status
ORDER BY ip_type, status;

-- ============================================================
-- View: All reserved IPs with time elapsed since reservation
-- ============================================================
CREATE OR REPLACE VIEW v_reserved_ips AS
SELECT
    id,
    ip_address,
    ip_type,
    service_id,
    reserved_at,
    EXTRACT(EPOCH FROM (NOW() - reserved_at))::INT AS reserved_seconds_ago
FROM ip_pool
WHERE status = 'reserved';
