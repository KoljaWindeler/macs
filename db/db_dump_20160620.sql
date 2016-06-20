DROP TABLE access;

CREATE TABLE `access` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `user_id` int(11) NOT NULL,
  `mach_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`),
  KEY `id_2` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=69 DEFAULT CHARSET=latin1;

INSERT INTO access VALUES("11","35","33");
INSERT INTO access VALUES("12","35","37");
INSERT INTO access VALUES("13","35","41");
INSERT INTO access VALUES("14","35","44");
INSERT INTO access VALUES("15","35","46");
INSERT INTO access VALUES("16","36","33");
INSERT INTO access VALUES("17","36","35");
INSERT INTO access VALUES("18","36","37");
INSERT INTO access VALUES("19","36","41");
INSERT INTO access VALUES("20","36","44");
INSERT INTO access VALUES("21","37","33");
INSERT INTO access VALUES("22","37","37");
INSERT INTO access VALUES("23","37","38");
INSERT INTO access VALUES("24","37","41");
INSERT INTO access VALUES("25","37","44");
INSERT INTO access VALUES("26","38","35");
INSERT INTO access VALUES("27","38","41");
INSERT INTO access VALUES("28","38","44");
INSERT INTO access VALUES("29","39","41");
INSERT INTO access VALUES("30","39","44");
INSERT INTO access VALUES("31","39","45");
INSERT INTO access VALUES("38","41","33");
INSERT INTO access VALUES("39","41","41");
INSERT INTO access VALUES("40","41","44");
INSERT INTO access VALUES("41","41","45");
INSERT INTO access VALUES("42","41","46");
INSERT INTO access VALUES("43","42","36");
INSERT INTO access VALUES("44","42","41");
INSERT INTO access VALUES("45","42","44");
INSERT INTO access VALUES("46","43","33");
INSERT INTO access VALUES("47","43","41");
INSERT INTO access VALUES("48","43","44");
INSERT INTO access VALUES("49","44","33");
INSERT INTO access VALUES("50","44","41");
INSERT INTO access VALUES("51","44","47");
INSERT INTO access VALUES("52","45","41");
INSERT INTO access VALUES("53","46","41");
INSERT INTO access VALUES("54","47","33");
INSERT INTO access VALUES("55","47","36");
INSERT INTO access VALUES("56","47","41");
INSERT INTO access VALUES("57","48","41");
INSERT INTO access VALUES("58","49","36");
INSERT INTO access VALUES("59","49","41");
INSERT INTO access VALUES("60","49","47");
INSERT INTO access VALUES("61","49","51");
INSERT INTO access VALUES("62","50","41");
INSERT INTO access VALUES("63","51","41");



DROP TABLE log;

CREATE TABLE `log` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `timestamp` int(11) NOT NULL,
  `user_id` int(11) NOT NULL,
  `machine_id` int(11) NOT NULL,
  `event` text NOT NULL,
  `login_id` int(11) DEFAULT NULL,
  `usage` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=46 DEFAULT CHARSET=latin1;

INSERT INTO log VALUES("1","1455295294","44","47","Unlocked","","0");
INSERT INTO log VALUES("2","1455295297","44","47","Locked","","2");
INSERT INTO log VALUES("3","1455295776","44","47","Unlocked","","0");
INSERT INTO log VALUES("4","1455295782","44","47","Locked","","7");
INSERT INTO log VALUES("7","1455295786","44","47","Unlocked","","0");
INSERT INTO log VALUES("8","1455295961","44","47","Locked","","174");
INSERT INTO log VALUES("9","1455295968","44","47","Unlocked","","0");
INSERT INTO log VALUES("10","1455295969","44","47","Locked","","2");
INSERT INTO log VALUES("11","1455295974","0","47","Rejected 8298962","","0");
INSERT INTO log VALUES("12","1455296696","1","47","Rejected","","0");
INSERT INTO log VALUES("13","1455296705","1","47","Rejected","","0");
INSERT INTO log VALUES("14","1455296716","1","47","Rejected","","0");
INSERT INTO log VALUES("15","1455365038","0","47","Rejected 8298962","","0");
INSERT INTO log VALUES("16","1455365045","44","47","Unlocked","","0");
INSERT INTO log VALUES("17","1455365048","44","47","Locked","","3");
INSERT INTO log VALUES("20","1455365127","44","47","Unlocked","","0");
INSERT INTO log VALUES("21","1455365343","44","47","Locked","","216");
INSERT INTO log VALUES("22","1455365348","0","47","Rejected 8298962","","0");
INSERT INTO log VALUES("23","1455365356","44","47","Unlocked","","0");
INSERT INTO log VALUES("24","1455365358","44","47","Locked","","2");
INSERT INTO log VALUES("25","1455380641","0","47","Rejected 8298962","","0");
INSERT INTO log VALUES("28","1455380646","44","47","Unlocked","","0");
INSERT INTO log VALUES("29","1455380680","44","47","Locked","","35");
INSERT INTO log VALUES("30","1455467102","0","47","Rejected 8298962","","0");
INSERT INTO log VALUES("33","1455467109","44","47","Unlocked","","0");
INSERT INTO log VALUES("34","1455467401","44","47","Locked","","292");
INSERT INTO log VALUES("35","1466445799","56","0","User deleted","1","0");
INSERT INTO log VALUES("36","1466445804","34","0","User deleted","1","0");
INSERT INTO log VALUES("37","1466445807","40","0","User deleted","1","0");
INSERT INTO log VALUES("38","1466445811","53","0","User deleted","1","0");
INSERT INTO log VALUES("39","1466445814","59","0","User deleted","1","0");
INSERT INTO log VALUES("40","1466445817","58","0","User deleted","1","0");
INSERT INTO log VALUES("41","1466445821","57","0","User deleted","1","0");
INSERT INTO log VALUES("42","1466445824","55","0","User deleted","1","0");
INSERT INTO log VALUES("43","1466445827","54","0","User deleted","1","0");
INSERT INTO log VALUES("44","1466445898","0","49","Machine deleted","1","0");
INSERT INTO log VALUES("45","1466445954","1","0","User updated","1","0");



DROP TABLE mach;

CREATE TABLE `mach` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` text NOT NULL,
  `mach_nr` int(11) NOT NULL,
  `desc` text NOT NULL,
  `last_seen` int(11) NOT NULL,
  `active` int(1) NOT NULL,
  `version` int(11) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=52 DEFAULT CHARSET=latin1;

INSERT INTO mach VALUES("3","Band saw 2","905","Fine blade","2","0","-1");
INSERT INTO mach VALUES("4","Band saw 3","903","Fine blade","1","0","-1");
INSERT INTO mach VALUES("5","CNC1","914","Gizzmo","1446786040","0","-1");
INSERT INTO mach VALUES("6","CNC2","901","Gizzmo","1","0","-1");
INSERT INTO mach VALUES("7","gizzmo","900","Gizzmo","1","0","-1");
INSERT INTO mach VALUES("8","Locker1","900","Gizzmo2","1","0","-1");
INSERT INTO mach VALUES("16","SawStop","906","Saw Stop Table Saw","0","0","-1");
INSERT INTO mach VALUES("17","Panel Saw","907","Safety-Speed Panel Saw","0","0","-1");
INSERT INTO mach VALUES("18","Miter Saw","915","Dewalt Sliding Compoud Miter Saw","1446784806","0","-1");
INSERT INTO mach VALUES("19","Lathe","908","11x26 Metal Lathe","0","0","-1");
INSERT INTO mach VALUES("20","CNC Router","909","CNC Router","1446694286","0","-1");
INSERT INTO mach VALUES("21","Finger Chopa","910","Very Very Mean Finger Chopper","0","0","-1");
INSERT INTO mach VALUES("23","Laser 1","911","Laser Engraver #1","0","0","-1");
INSERT INTO mach VALUES("26","12th test","900","test kolja","0","0","-1");
INSERT INTO mach VALUES("27","12th test 2","900","new 12er","0","0","-1");
INSERT INTO mach VALUES("28","asd","902","asdasd","0","0","-1");
INSERT INTO mach VALUES("29","Fusion Saw","916","Cuts through solid Krell Metal","1446699467","0","-1");
INSERT INTO mach VALUES("30","Flux Cap","912","Buy one last year","1444442349","0","-1");
INSERT INTO mach VALUES("31","Test Printer","904","A4 Printer","0","0","-1");
INSERT INTO mach VALUES("32","Somemachine","913","12345","0","0","-1");
INSERT INTO mach VALUES("33","Laser1","1","Laser Engraver 1","1449290029","1","-1");
INSERT INTO mach VALUES("34","Laser2","917","Laser Engraver 2","1448811649","0","-1");
INSERT INTO mach VALUES("35","Table Saw","17","SawStop Table Saw","1452439506","1","20151231");
INSERT INTO mach VALUES("36","Metal Lathe1","2","Grizzly 11x26 Lathe","1449291042","1","-1");
INSERT INTO mach VALUES("37","Wood Lathe","5","14\" Wood Lathe","1452439461","1","20151231");
INSERT INTO mach VALUES("38","Mill1","6","Bridgeport Mill","0","1","-1");
INSERT INTO mach VALUES("39","Router Table","7","Router Table","1447449270","1","-1");
INSERT INTO mach VALUES("40","Zenbot","918","CNC Router","1446907055","0","-1");
INSERT INTO mach VALUES("41","Panel Saw","88","Safety Saw C4 Panel Saw","1449309236","1","-1");
INSERT INTO mach VALUES("42","Planer","10","15\" Grizzly Planer","0","1","-1");
INSERT INTO mach VALUES("43","Jointer","11","Grizzly 8\" Jointer","0","1","-1");
INSERT INTO mach VALUES("44","Front Door","50","Front Door Access","0","1","-1");
INSERT INTO mach VALUES("45","Tool Room","51","Tool Room Access","0","1","-1");
INSERT INTO mach VALUES("46","Miter Saw","12","Dewalt SCMS","0","1","-1");
INSERT INTO mach VALUES("47","Drill Press","3","Drill Press","1455544028","1","20160213");
INSERT INTO mach VALUES("48","Hitachi CMS","73","Miter saw","1452439639","1","20151231");
INSERT INTO mach VALUES("49","new","919","new","1448800952","0","-1");
INSERT INTO mach VALUES("50","Sander","8","Sanders","1449238451","1","-1");
INSERT INTO mach VALUES("51","Demo","9","Demonstration Unit","1449238644","1","-1");



DROP TABLE update_available;

CREATE TABLE `update_available` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mach_id` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1476 DEFAULT CHARSET=latin1;

INSERT INTO update_available VALUES("1416","3");
INSERT INTO update_available VALUES("1417","4");
INSERT INTO update_available VALUES("1418","5");
INSERT INTO update_available VALUES("1419","6");
INSERT INTO update_available VALUES("1420","7");
INSERT INTO update_available VALUES("1421","8");
INSERT INTO update_available VALUES("1422","16");
INSERT INTO update_available VALUES("1423","17");
INSERT INTO update_available VALUES("1424","18");
INSERT INTO update_available VALUES("1425","19");
INSERT INTO update_available VALUES("1426","20");
INSERT INTO update_available VALUES("1427","21");
INSERT INTO update_available VALUES("1428","23");
INSERT INTO update_available VALUES("1429","26");
INSERT INTO update_available VALUES("1430","27");
INSERT INTO update_available VALUES("1431","28");
INSERT INTO update_available VALUES("1432","29");
INSERT INTO update_available VALUES("1433","30");
INSERT INTO update_available VALUES("1434","31");
INSERT INTO update_available VALUES("1435","32");
INSERT INTO update_available VALUES("1437","34");
INSERT INTO update_available VALUES("1442","39");
INSERT INTO update_available VALUES("1443","40");
INSERT INTO update_available VALUES("1445","42");
INSERT INTO update_available VALUES("1446","43");
INSERT INTO update_available VALUES("1452","49");
INSERT INTO update_available VALUES("1454","51");
INSERT INTO update_available VALUES("1460","38");
INSERT INTO update_available VALUES("1463","45");
INSERT INTO update_available VALUES("1464","46");
INSERT INTO update_available VALUES("1465","48");
INSERT INTO update_available VALUES("1467","35");
INSERT INTO update_available VALUES("1469","37");
INSERT INTO update_available VALUES("1470","41");
INSERT INTO update_available VALUES("1471","44");
INSERT INTO update_available VALUES("1472","36");
INSERT INTO update_available VALUES("1473","47");
INSERT INTO update_available VALUES("1474","50");
INSERT INTO update_available VALUES("1475","33");



DROP TABLE user;

CREATE TABLE `user` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` text NOT NULL,
  `login` text NOT NULL,
  `hash` text NOT NULL,
  `badge_id` text NOT NULL,
  `email` text NOT NULL,
  `last_seen` int(11) NOT NULL,
  `active` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`),
  KEY `id_2` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=60 DEFAULT CHARSET=latin1;

INSERT INTO user VALUES("1","Admin1","admin","21232f297a57a5a743894a0e4a801fc3","1234567890","Admin@internet.com","1443690569","1");
INSERT INTO user VALUES("35","GregR","","d41d8cd98f00b204e9800998ecf8427e","11842381","unknown@X.com","0","1");
INSERT INTO user VALUES("36","GlenO","","d41d8cd98f00b204e9800998ecf8427e","11859318","unknown@X.com","0","1");
INSERT INTO user VALUES("37","BlakeB","","d41d8cd98f00b204e9800998ecf8427e","11848282","unknown@X.com","0","1");
INSERT INTO user VALUES("38","JohnB","","d41d8cd98f00b204e9800998ecf8427e","11856004","unknown@X.com","0","1");
INSERT INTO user VALUES("39","EliseEb","","d41d8cd98f00b204e9800998ecf8427e","11843996","unknown@X.com","0","1");
INSERT INTO user VALUES("41","JeanieJ","","d41d8cd98f00b204e9800998ecf8427e","11858220","unknown@X.com","0","1");
INSERT INTO user VALUES("42","AndrewJ","","d41d8cd98f00b204e9800998ecf8427e","11854380","unknown@X.com","0","1");
INSERT INTO user VALUES("43","RudM","","d41d8cd98f00b204e9800998ecf8427e","11852054","unknown@X.com","0","1");
INSERT INTO user VALUES("44","KoljaW","","d41d8cd98f00b204e9800998ecf8427e","2722341","unknown@x.com","0","1");
INSERT INTO user VALUES("45","KathrynB","","d41d8cd98f00b204e9800998ecf8427e","2722147","unknown@x.com","0","1");
INSERT INTO user VALUES("46","JessieJ","","d41d8cd98f00b204e9800998ecf8427e","2724893","unknown@x.com","0","1");
INSERT INTO user VALUES("47","Guest1","","d41d8cd98f00b204e9800998ecf8427e","2728284","unknown@x.com","0","1");
INSERT INTO user VALUES("48","Guest2","","d41d8cd98f00b204e9800998ecf8427e","2722221","unknown@x.com","0","1");
INSERT INTO user VALUES("49","Guest3","","d41d8cd98f00b204e9800998ecf8427e","2722614","unknown@x.com","0","1");
INSERT INTO user VALUES("50","Guest4","","d41d8cd98f00b204e9800998ecf8427e","2744381","unknown@x.com","0","1");
INSERT INTO user VALUES("51","Guest5","","d41d8cd98f00b204e9800998ecf8427e","2736279","unknown@x.com","0","1");



DROP TABLE wifi;

CREATE TABLE `wifi` (
  `id` int(11) NOT NULL,
  `ssid` text NOT NULL,
  `pw` text NOT NULL,
  `type` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

INSERT INTO wifi VALUES("0","macs","6215027094","3");
INSERT INTO wifi VALUES("1","shop","abcdefgh","3");
INSERT INTO wifi VALUES("2","ajlokert","qweqweqwe","3");



