function togglekey(keys, activate, deactivate) {
	var enabled = false;
	key(keys, function() {
		if (enabled) {
			deactivate();
			enabled = false;
		} else {
			activate();
			enabled = true;
		}
	});
}
