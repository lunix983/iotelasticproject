create database sensor_metadata;

use sensor_metadata;

create table sensor_type(sensor_type_id int, sensor_type varchar(30));

create table location(location_id int, location_description varchar(500),latitude double, longitude double); 

create table sensors(sensor_id int primary key, sensor_type_id int, location_id int);


insert into sensor_type values(1, 'dht11');

insert into location values(1, 'Casa Adelfia', 41.047818, 16.9147363);

insert into sensors values(1, 1, 1);
