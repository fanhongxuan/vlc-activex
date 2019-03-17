/*
<OWNER> = revolunet
<ORGANIZATION> = revolunet - Julien Bouquillon
<YEAR> = 2008

Copyright (c) 2008, revolunet
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met :


 Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
 Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 
 Neither the name of the <ORGANIZATION> nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES ; LOSS OF USE, DATA, OR PROFITS ; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/



var sliders = new Array();

function Slider(sliderID, width, height) {

	this.position=0;
	sliders[sliderID] = this;
 
	var tgt = document.getElementById(sliderID);
	this.tgt = tgt;
	if(!tgt) return;
	//alert((width - 2) + "px");
	tgt.style.width = (width - 2) + "px";
	tgt.style.height = height + "px";
	tgt.style.backgroundColor = "silver";
	tgt.style.border = "1px solid black";
	tgt.style.cursor = "pointer";
	tgt.style.textAlign="left";
	tgt.style.zIndex="1";
	//tgt.style.position = "absolute";
	//tgt.style.left = "0px";
	
	var thumb = document.createElement("div");
	thumb.setAttribute("id", sliderID + "_thumb");
	thumb.slider = this;
	
	thumb.setAttribute("style", "float:left");
	
	thumb.style.display = "inline";
	thumb.style.width = "5px";
	thumb.style.height = height + "px";
	
	
	//alert(	tgt.style.height );
	//alert(	thumb.style.height );
	//thumb.style.zIndex="1000";
	
	thumb.style.backgroundColor = "#666666";
	thumb.style.borderLeft = "0px solid black";
	thumb.style.borderRight = "0px solid black";
	thumb.style.cursor = "pointer";
	thumb.style.position = "relative";
	
	thumb.dragging = false;
	tgt.onmousedown = function() {eval("Slider_click('" + sliderID + "')");}
	thumb.onmousedown = function() {eval("Slider_startDrag('" + sliderID + "')");}
	thumb.onmousemove = function() {eval("Slider_Drag('" + sliderID + "')");}
	tgt.onmousemove = function() {eval("Slider_Drag('" + sliderID + "')");}
	//thumb.onmouseup = function() {eval("Slider_stopDrag('" + sliderID + "')");}
	//thumb.onmouseout = function() {eval("Slider_stopDrag('" + sliderID + "')");}
	//tgt.onmouseup = this.stopDrag;
	//tgt.innerHTML+="";
	tgt.appendChild(thumb);
	
//	thumb.innerHTML+="-";
	
 
	
//	alert(tgt.innerHTML);
	
	
	
	this.thumb = document.getElementById(sliderID + "_thumb");
 
	//this.thumb.style.top="50px";
	//this.thumb.style.left="50px";
	//alert(this.thumb.style.left);
	//alert(this.thumb);

	document.onmousemove = doSomething;
//	alert(window.document.onmousemove);
}


Slider.prototype.setPosition = function(position) {
	this.position = position;
	Slider_setXFromPosition(this.tgt.id, position);
}


function getX( oElement )
{
	var iReturnValue = 0;
	while( oElement != null ) {
	iReturnValue += oElement.offsetLeft;
	//alert(oElement.offsetLeft);
	oElement = oElement.offsetParent;
	}
	return iReturnValue;
}


function Slider_click(id, e) {
	//var tgt =  sliders[id]
	//var lbase = getX(tgt.tgt);
	//var relpos = parseInt(event.clientX - lbase - (parseInt((tgt.thumb.style.width.replace("px",""))) / 2));
 	//tgt.thumb.style.left = relpos + "px";


	//alert(e);
	Slider_setXfromevent(id);
	Slider_gotnewposition(id);
}

function Slider_startDrag(id) {
	var tgt =  sliders[id];
	// var lbase = getX(tgt.tgt);
	// var offset = parseInt((tgt.thumb.style.width.replace("px","")));
	//var relpos = parseInt(event.clientX - lbase - offset);
 	//tgt.thumb.style.left = relpos + "px";
	document.onmouseup = function() {eval("Slider_stopDrag('" + id + "')");}
	tgt.thumb.dragging = true;
}

function Slider_Drag(id) {
	
	var tgt =  sliders[id];
	if (!tgt.thumb.dragging) return;
	Slider_setXfromevent(id);

}

function Slider_setXfromevent(id) {
	var event = window.event||window.Event;
	var tgt =  sliders[id];
	var lbase = getX(tgt.tgt);
	var thumb_width = parseInt(tgt.thumb.style.width.replace("px",""))
	var offset = parseInt(thumb_width / 2);

	var relpos = parseInt(posx - lbase - offset);
	var bar_width = parseInt(tgt.tgt.style.width.replace("px",""));
	if (relpos >= (bar_width - thumb_width)) relpos = bar_width - thumb_width;
	if (relpos < 0) relpos=0;
	
	Slider_setX(id, relpos);
 	
}

function Slider_setXFromPosition(id, position) {
	var tgt =  sliders[id];
	var lbase = getX(tgt.tgt);
	var thumb_width = parseInt(tgt.thumb.style.width.replace("px",""))
	var bar_width = parseInt(tgt.tgt.style.width.replace("px",""));
	var offset = parseInt(thumb_width / 2);
	var relpos = parseInt((position * bar_width) - offset );
	if (relpos >= (bar_width - thumb_width)) relpos = bar_width - thumb_width;
	if (relpos < 0) relpos=0; 
	Slider_setX(id, relpos);
}

function Slider_setX(id, x) {
	var tgt =  sliders[id];
	tgt.thumb.style.left = x + "px";
}



function Slider_stopDrag(id) {
	//alert(event.clientX);
//	alert(this.thumb);
	//this.thumb.style.left = event.clientX;
	Slider_setXfromevent(id);
	var tgt =  sliders[id];
	tgt.thumb.dragging = false;
	document.onmouseup = null;
	Slider_gotnewposition(id);
}

function Slider_gotnewposition(id) {
	var tgt =  sliders[id];
	var thumb_pos = parseInt(tgt.thumb.style.left.replace("px",""));
	if (thumb_pos < 0) thumb_pos=0;
	if (thumb_pos > parseInt(tgt.tgt.style.width.replace("px",""))) thumb_pos=parseInt(tgt.tgt.style.width.replace("px",""));
	var value = thumb_pos  / parseInt(tgt.tgt.style.width.replace("px",""));
	tgt.position = value;
	//alert(value);
	if (tgt.onNewPosition) tgt.onNewPosition();

}



var posx = 0;
var posy = 0;
 
function doSomething(e) {
	
	if (!e) var e = window.event;
	if (e.pageX || e.pageY) 	{
		posx = e.pageX;
		posy = e.pageY;
	}
	else if (e.clientX || e.clientY) 	{
		posx = e.clientX + document.body.scrollLeft
			+ document.documentElement.scrollLeft;
		posy = e.clientY + document.body.scrollTop
			+ document.documentElement.scrollTop;
	}
	// posx and posy contain the mouse position relative to the document
	// Do something with this information
	

}

//SimpleSliderLibReceived();
 