INSERT INTO membership_directory(short_desc, long_desc, filename, last_update, num_records)
SELECT 'DXPED', 'Active DXpeditions (Club Log)', 'clublog-expeditions', strftime('%Y%m%d', 'now'), NULL
WHERE NOT EXISTS (SELECT 1 FROM membership_directory WHERE short_desc = 'DXPED');
