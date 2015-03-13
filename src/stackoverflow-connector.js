/*
// PebbleOverflow
// Copyright (c) 2015, Adrian Shatte
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
*/

// handles external api requests
var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// reference to the user ID for StackOverflow
var userID = "";

// Retrieves the username, total reputation, and daily reputation gain from StackOverflow based on user ID
function getUserInfo() {
	if (userID.length > 0) {
		var url = 'https://api.stackexchange.com/2.2/users/' + userID + '?order=desc&sort=reputation&site=stackoverflow';
	  
		// Send request to StackOverflow
	    xhrRequest(url, 'GET', 
		    function(responseText) {
		        // responseText contains a JSON object with user info
		        var json = JSON.parse(responseText);
				
		        // Get the username
		        var username = json.items[0].display_name;
		  
		        // Get the total rep
		        var totalrep = json.items[0].reputation.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
				
		        // Get the daily rep change
		        var repgained;
		        if (json.items[0].reputation_change_day > 0) {
			        repgained = "+ " + json.items[0].reputation_change_day + " today";
		        } else {
			        repgained = "";
		        }
				
		        // Assemble dictionary using the keys from main.c
		        var dictionary = {
			        'KEY_USERNAME': username,
			        'KEY_TOTALREP': totalrep,
			        'KEY_REPGAINED': repgained
		        };
				
		        // Send to Pebble
		        Pebble.sendAppMessage(dictionary,
			        function(e) {
			            console.log("StackOverflow info sent to Pebble successfully!");
			        },
			        function(e) {
			            console.log("Error sending StackOverflow info to Pebble!");
			        }
		        );
		    }   
	    );
    }
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
    function(e) {
        console.log('PebbleKit JS ready!');
        userID = localStorage.getItem('uID');
        if (userID.length > 0 || userID !== null || userID !== undefined) getUserInfo();
    }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
    function(e) {
        console.log('AppMessage received!');
        getUserInfo();
    }                     
);

// Listen for when user opens config window
Pebble.addEventListener('showConfiguration', 
    function(e) {
        // Show config page
        Pebble.openURL('http://adrianshatte.com/ApplicationBackends/pebble/pebbleOverflow/configuration.html?uID=' + userID);
    }
); 

// Listen for when user closes config window
Pebble.addEventListener('webviewclosed',
    function(e) {
        var configuration = decodeURIComponent(e.response);
        var json = JSON.parse(configuration);
        console.log('Configuration window returned: ' + json.userID);
        userID = json.userID;
        localStorage.setItem('uID', userID);
        getUserInfo();
    }
);