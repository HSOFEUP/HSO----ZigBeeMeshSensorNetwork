SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

CREATE SCHEMA IF NOT EXISTS `store` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci ;
USE `store` ;

-- -----------------------------------------------------
-- Table `store`.`Node`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `store`.`Node` (
  `id_mac` VARCHAR(16) NOT NULL ,
  `time` INT(11) NOT NULL ,
  `status_read` INT NOT NULL DEFAULT -1 ,
  `descriptor` VARCHAR(45) NOT NULL ,
  PRIMARY KEY (`id_mac`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_general_ci;


-- -----------------------------------------------------
-- Table `store`.`Sensor`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `store`.`Sensor` (
  `id_sensor` INT UNSIGNED NOT NULL ,
  `descriptor` TEXT NOT NULL ,
  PRIMARY KEY (`id_sensor`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_general_ci;


-- -----------------------------------------------------
-- Table `store`.`Read`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `store`.`Read` (
  `id_reads` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `id_node` VARCHAR(16) NOT NULL ,
  `id_sensor` INT UNSIGNED NOT NULL ,
  `value_1` FLOAT NOT NULL ,
  `value_2` FLOAT NULL ,
  `value_3` FLOAT NULL ,
  `timestamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ,
  PRIMARY KEY (`id_reads`) ,
  INDEX `fk_Reads_1` (`id_sensor` ASC) ,
  INDEX `fk_Reads_2` (`id_node` ASC) ,
  CONSTRAINT `fk_Reads_1`
    FOREIGN KEY (`id_sensor` )
    REFERENCES `store`.`Sensor` (`id_sensor` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Reads_2`
    FOREIGN KEY (`id_node` )
    REFERENCES `store`.`Node` (`id_mac` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_general_ci;


-- -----------------------------------------------------
-- Table `store`.`User`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `store`.`User` (
  `username` VARCHAR(45) NOT NULL ,
  `password` VARCHAR(45) NOT NULL ,
  `email` VARCHAR(100) NOT NULL ,
  PRIMARY KEY (`username`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_general_ci;


-- -----------------------------------------------------
-- Table `store`.`Store`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `store`.`Store` (
  `id_store` VARCHAR(16) NOT NULL ,
  `name_store` TEXT NULL ,
  `web_service` TEXT NOT NULL ,
  PRIMARY KEY (`id_store`) )
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_general_ci;


-- -----------------------------------------------------
-- Table `store`.`Node_has_Sensor`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `store`.`Node_has_Sensor` (
  `Node_id_mac` VARCHAR(16) NOT NULL ,
  `Sensor_id_sensor` INT UNSIGNED NOT NULL ,
  PRIMARY KEY (`Node_id_mac`, `Sensor_id_sensor`) ,
  INDEX `fk_Node_has_Sensor_Sensor1` (`Sensor_id_sensor` ASC) ,
  INDEX `fk_Node_has_Sensor_Node1` (`Node_id_mac` ASC) ,
  CONSTRAINT `fk_Node_has_Sensor_Node1`
    FOREIGN KEY (`Node_id_mac` )
    REFERENCES `store`.`Node` (`id_mac` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Node_has_Sensor_Sensor1`
    FOREIGN KEY (`Sensor_id_sensor` )
    REFERENCES `store`.`Sensor` (`id_sensor` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_general_ci;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
