# IP Inventory REST API

An IP address inventory management API built with **C++20**, **Drogon**, and **PostgreSQL** on Debian 13 OS

---

## Project Structure

```
ip-inventory/
├── CMakeLists.txt
├── config/
│   └── config.json              # Drogon + DB config
├── include/
|   |── app
│   ├── common/Types.h           # Shared types & response helpers
│   ├── controllers/*
│   ├── repository/*
│   ├── services/*
│   │
│   └── utils/
│       ├── IpUtils.h
│       └── JsonValidator.h
├── src/
│   ├── main.cpp
|   |── app
│   ├── controllers/             # One .cpp per endpoint
│   ├── repository/*
│   ├── services/*
│   └── utils/
├── sql/
│   └── schema.sql               # PostgreSQL schema + indexes + views
├── swagger/
│   └── openapi.yaml             # OpenAPI 3.0 specification
├── gui/
│   └── index.html               # Single-page management UI
└── scripts/
    ├── install_and_build.sh     # Debian dependency install + cmake build
    └── setup_db.sh              # PostgreSQL DB + user + schema setup
```

---

## Quick Start (Debian 13)

### 1. Install dependencies and build

This installs all required packages and clones/builds Drogon from source.

### 2. Set up the database

### 3. Configure

Edit `config/config.json` — update the `db_clients` section with your PostgreSQL credentials.

```json
{
  "db_clients": [
    {
      "host": "127.0.0.1",
      "port": 5432,
      "dbname": "db_name_placeholder",
      "user": "user_placeholder",
      "passwd": "pass_placeholder"
    }
  ],
  "ip_inventory": {
    "reservation_timeout_seconds": 300,
    "expiry_check_interval_seconds": 30
  }
}
```

### 4. Run

```bash
./build/ip_inventory
```

### 5. Open the GUI

Open `gui/index.html` in your browser.  
Set the API Base URL to `http://localhost:8080` and click **TEST CONNECTION**.

---

## API Reference

### POST `/ip-inventory/ip-pool`

Add IP addresses to the inventory pool (status: **free**).

```json
{
  "ipAddresses": [
    { "ip": "95.44.73.19", "ipType": "IPv4" },
    { "ip": "2a01:05a9:01a4:095c:0000:0000:0000:0001", "ipType": "IPv6" }
  ]
}
```

Response `200 OK`: `{ "statusCode": "0", "statusMessage": "Successful operation. OK" }`

---

### POST `/ip-inventory/reserve-ip`

Reserve a free IP for a serviceId.  
`ipType` may be `"IPv4"`, `"IPv6"`, or `"Both"`.

```json
{ "serviceId": "xxxyyy", "ipType": "Both" }
```

Response: `{ "ipAddresses": [ { "ip": "...", "ipType": "IPv4" }, ... ] }`

> **Auto-expiry**: if `assign-ip-serviceId` is not called within
> `reservation_timeout_seconds` (default 300 s), the reservation is
> automatically released by the background job.

---

### POST `/ip-inventory/assign-ip-serviceId`

Assign IPs to a service (free/reserved → assigned).

```json
{
  "serviceId": "xxxyyy",
  "ipAddresses": [{ "ip": "95.44.73.19" }, { "ip": "2a01:..." }]
}
```

---

### POST `/ip-inventory/terminate-ip-serviceId`

Release IPs from a service (assigned → free).

```json
{
  "serviceId": "xxxyyy",
  "ipAddresses": [{ "ip": "95.44.73.19" }]
}
```

---

### POST `/ip-inventory/serviceId-change`

Reassign all assigned IPs from one service to another.

```json
{ "serviceIdOld": "xxxyyy", "serviceId": "zzzppp" }
```

---

### GET `/ip-inventory/serviceId?serviceId=xxxyyy`

Returns all currently assigned IPs for the given service.

```json
{
  "ipAddresses": [
    { "ip": "95.44.73.19", "ipType": "IPv4" },
    { "ip": "2a01:...", "ipType": "IPv6" }
  ]
}
```

---

## Architecture

```
HTTP Request
     │
     ▼
 Controller (validates JSON, parses params)
     │
     ▼
 Service Layer (business logic, SQL)
     │
     ▼
 Drogon DB Client (async PostgreSQL pool)
     │
     ▼
 PostgreSQL (ip_pool table + audit log)
```

### Audit Log

Every state change (add, reserve, assign, terminate, release, service_change) is written to `ip_audit_log` with timestamp and JSONB details.

---

## Swagger / OpenAPI

The full OpenAPI 3.0 specification is at `swagger/openapi.yaml`.

View it in Swagger UI:

```bash
docker run -p 8081:8080 \
  -e SWAGGER_JSON=/api/openapi.yaml \
  -v $(pwd)/swagger:/api \
  swaggerapi/swagger-ui
```

Then open http://localhost:8081

---

## Database Schema Overview

| Table          | Purpose                                   |
| -------------- | ----------------------------------------- |
| `ip_pool`      | Master inventory — one row per IP address |
| `ip_audit_log` | Immutable log of every state transition   |

| View                | Purpose                                 |
| ------------------- | --------------------------------------- |
| `v_ip_pool_summary` | Count by status × ip_type               |
| `v_reserved_ips`    | Active reservations with age in seconds |

---

## Configuration Reference

| Key                                          | Default | Description                               |
| -------------------------------------------- | ------- | ----------------------------------------- |
| `ip_inventory.reservation_timeout_seconds`   | `300`   | Seconds before a reservation auto-expires |
| `ip_inventory.expiry_check_interval_seconds` | `30`    | How often the expiry job runs             |
| `app.threads_num`                            | `4`     | Drogon I/O threads                        |
| `listeners[0].port`                          | `8080`  | HTTP listen port                          |
