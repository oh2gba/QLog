# Club list "DXPED" – active DXpeditions

QLog has a built-in club list **DXPED** with the DXpeditions that are active
according to [Club Log](https://clublog.org/expeditions.php). It is listed in
*Settings → Callbook & Lists → Club Lists* like any other list and can be used
in the same places: the DX Cluster filter, the WSJT-X filter and the Member
part of an alert rule.

## Why

Most DXpeditions upload to LoTW only after they are over. A filter or alert
rule limited to *LoTW* users therefore hides exactly the stations a DXer is
waiting for. Tick **LoTW** and **DXPED** together and the rule reads
"LoTW users, or an active DXpedition".

## Details

- Source: the public Club Log expedition list, fetched by QLog directly.
- A DXpedition counts as active for 12 months after its last QSO; older
  entries are dropped when the list is imported.
- The list is refreshed together with the other club lists, and with
  `qlog --force-update`.
- In the membership details of a callsign the list shows the date of the last
  QSO reported by Club Log as the member ID.
