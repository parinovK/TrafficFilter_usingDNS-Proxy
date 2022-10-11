# TrafficFilter (Using DNS, Proxy & Root Certificates)
Фильтрация трафика через DNS, Proxy и корневые сертификаты  
/*  
## Программа предлагает:  
[+]Установку сертификатов корневого уровня  
[+]Настройку Proxy + автоматическое включение/выключение Proxy в зависимости от текущего состояния переключателя  
[+]Настройку DNS. IPv4  
Всё это нужно, например, для фильтрации трафика в школе.
*/  

/*  
## Принцип работы:  
1)Установка сертификатов происходит при помощи certmgr, при его вызове в командную строку передаются параметры - например, имя сертификата и тип сертификата. Сам certmgr хранится в памяти в бинарном виде  
2)DNS настраивается при помощи простого bat-ника. Bat-ник создается во время выполнения программы и удаляется после отработки  
3)Изменение прокси происходит путем изменения значений в ветках реестра. Также проверяется был ли включен/выключен прокси, для соответсвующего включения/выключения прокси автоматически.  
*/
