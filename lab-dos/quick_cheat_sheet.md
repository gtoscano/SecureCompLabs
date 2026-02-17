# Quick instructor cheat-sheet (1 slide)

**Baseline**

* `docker stats`
* `ss -s`, `ss -tuln`
* `ss -tan | awk...` (port distribution)
* `iftop`

**Stress**

* `wrk -t4 -c300 -d45s http://target/`

**Defend**

* Nginx: `limit_req_*`, `limit_conn_*`, timeouts
* Host: `tcp_syncookies`, backlog tuning

**Verify**

* Repeat `wrk`
* Compare metrics table

