#!/usr/bin/env bash
# ============================================================
# scripts/setup_db.sh
# Creates the PostgreSQL database, user, and schema.
# ============================================================
set -euo pipefail

DB_NAME="${DB_NAME:-data}"
DB_USER="${DB_USER:-ross}"
DB_PASS="${DB_PASS:-ross}"
DB_HOST="${DB_HOST:-localhost}"
DB_PORT="${DB_PORT:-5432}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SCHEMA="${SCRIPT_DIR}/../sql/schema.sql"

echo "=== Creating PostgreSQL user and database ==="
sudo -u postgres psql <<SQL
DO \$\$
BEGIN
  IF NOT EXISTS (SELECT FROM pg_roles WHERE rolname = '${DB_USER}') THEN
    CREATE USER ${DB_USER} WITH PASSWORD '${DB_PASS}';
  END IF;
END
\$\$;

SELECT 'CREATE DATABASE ${DB_NAME} OWNER ${DB_USER}'
WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = '${DB_NAME}')\gexec

GRANT ALL PRIVILEGES ON DATABASE ${DB_NAME} TO ${DB_USER};
SQL

echo "=== Applying schema ==="
PGPASSWORD="${DB_PASS}" psql \
    -h "${DB_HOST}" -p "${DB_PORT}" \
    -U "${DB_USER}" -d "${DB_NAME}" \
    -f "${SCHEMA}"

echo "=== Database setup complete ==="
echo "  Host:     ${DB_HOST}:${DB_PORT}"
echo "  Database: ${DB_NAME}"
echo "  User:     ${DB_USER}"
