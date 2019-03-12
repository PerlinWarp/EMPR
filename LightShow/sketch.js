let mic;
var button, micOn;

var volHistory = [];

function setup() {
  createCanvas(640, 480);

  //Microphone setup
  mic = new p5.AudioIn();
  mic.start();
  micOn = true;

  button = createButton('toggle');
  button.mousePressed(toggleMic);
}

function draw() {
	background(0);
	if(micOn){
		var vol = mic.getLevel();
		console.log(vol);
		volHistory.push(vol);
	}

	stroke(255);
	noFill();
	beginShape();
	for(var i = 0; i < volHistory.length; i++){
		var y = map(volHistory[i], 0, 1, height, 0);
		vertex(i,y);

	}
	endShape();

	if (volHistory.length > width){
		//Remove one element from the start of the array
		volHistory.splice(0,1);
	}
}

function toggleMic(){
	if (micOn){
		mic.stop();
		micOn = false;
	}else{
		mic.start();
		micOn = true;
	}

}

// Needed to get the audio working on chrome
function touchStarted() { getAudioContext().resume(); } 
