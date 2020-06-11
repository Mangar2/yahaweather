#pragma once

const char* cssFile = R"cssfile(
body {font-family: Arial, Helvetica, sans-serif;}
* {box-sizing: border-box;}

input:not([type=submit]) {
  width: 100%;
  padding: 12px;
  border: 1px solid #ccc;
  border-radius: 4px;
  box-sizing: border-box;
  margin-top: 6px;
  margin-bottom: 16px;
  resize: vertical;
}

input[type=submit] {
  background-color: #4CAF50;
  color: white;
  padding: 12px 20px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

input.tb {
  padding: 10px 15px 11px !important;
  font-size: 18px !important;
  background-color: #7E9077;
  width: 100px;
  height: 44px;
  font-weight: bold;
  text-shadow: 1px 1px #7E9077;
  color: #ffffff;
  border-radius: 5px;
  -moz-border-radius: 5px;
  -webkit-border-radius: 5px;
  border: 1px solid #8A9287;
  cursor: pointer;
  box-shadow: 0 1px 0 rgba(255, 255, 255, 0.5) inset;
  -moz-box-shadow: 0 1px 0 rgba(255, 255, 255, 0.5) inset;
  -webkit-box-shadow: 0 1px 0 rgba(255, 255, 255, 0.5) inset;
}

label.tb {
    display: inline-block;
    width: 150px;
    color: #888;
    line-height: 50px;
}

input[type=submit]:hover {
  background-color: #45a049;
}

.container {
  border-radius: 5px;
  background-color: #f2f2f2;
  padding: 20px;
}

.topnav {
  overflow: hidden;
  background-color: #333;
}

.topnav a {
  float: left;
  color: #f2f2f2;
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
  font-size: 17px;
}

.topnav a:hover {
  background-color: #ddd;
  color: black;
}

.topnav a.active {
  background-color: #4CAF50;
  color: white;
}

.sw {
    position: relative; width: 101px;
    -webkit-user-select:none; -moz-user-select:none; -ms-user-select: none;
    margin-bottom: 15px;
    margin-top: 10px;
}
.sw-checkbox {
    position: absolute;
    opacity: 0;
    pointer-events: none;
}
.sw-label {
    display: block; overflow: hidden; cursor: pointer;
    border: 2px solid #999999; border-radius: 5px;
}
.sw-inner {
    display: block; width: 200%; margin-left: -100%;
    transition: margin 0.3s ease-in 0s;
}
.sw-inner:before, .sw-inner:after {
    display: block; float: left; width: 50%; height: 30px; padding: 0; line-height: 30px;
    font-size: 14px; color: white; font-family: Trebuchet, Arial, sans-serif; font-weight: bold;
    box-sizing: border-box;
}
.sw-inner:before {
    content: "ON";
    padding-left: 10px;
    background-color: #34A7C1; color: #FFFFFF;
}
.sw-inner:after {
    content: "OFF";
    padding-right: 10px;
    background-color: #EEEEEE; color: #999999;
    text-align: right;
}
.sw-switch {
    display: block; width: 18px; margin: 6px;
    background: #FFFFFF;
    position: absolute; top: 0; bottom: 0;
    right: 67px;
    border: 2px solid #999999; border-radius: 20px;
    transition: all 0.3s ease-in 0s; 
}
.sw-checkbox:checked + .sw-label .sw-inner {
    margin-left: 0;
}
.sw-checkbox:checked + .sw-label .sw-switch {
    right: 0px; 
}
)cssfile";
