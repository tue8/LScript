/*
 * This is a test script 
 */

var tax = 30;
var robuxEarned = 50;
var robux = robuxEarned * (tax / 100);

if (robux >= 50)
	print "You earned 50 or more robux (" + robux + " robux) (after tax)";
else
	print "You earned less than 50 robux (" + robux + " robux) (after tax)";
