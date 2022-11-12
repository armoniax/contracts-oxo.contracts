# 1、创建分账
mpush amax.split addplan '["meta.book","6,MUSDT",true]' -p armoniaadmin
## plan_id = 5

# mpush amax.split setplan '["meta.book",5,[["meta.balance", 4500], ["meta.settle", 2500], ["meta.swap", 3000]]]' -p armoniaadmin

mpush meta.book setconf '["meta.conf","amax.split",5]' -p meta.book