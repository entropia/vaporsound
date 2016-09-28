$(function() {

	function do_update() {
		$.ajax({
			"url": "/api/v1/volume",
			"dataType": "text",
			"success": function(data) {
				$("#volume").slider("option", "value", data);
			}
		});

		setTimeout(do_update, 10000);
	}

	$(document).ready(function() {
		$("#volume").slider({
			min: 0,
			max: 100,
			step: 1,
			slide: function(event, ui) {
				$(this).find( ".ui-slider-handle" ).text( ui.value );
			},
			change: function(event, ui) {
				$(this).find( ".ui-slider-handle" ).text( ui.value );
			},
			stop: function(event, ui) {
				$.post("/api/v1/volume", "" + ui.value);
			}
		});

		do_update();
	});
});
