type ThrottleFunction<T extends (...args: any[]) => any> = {
	(...args: Parameters<T>): void;
	cancel: () => void;
};

export function throttle<T extends (...args: any[]) => any>(
	func: T,
	wait: number
): ThrottleFunction<T> {
	let timeout: ReturnType<typeof setTimeout> | null = null;
	let lastArgs: Parameters<T> | null = null;
	let lastCallTime = 0;

	function throttled(...args: Parameters<T>): void {
		const now = Date.now();

		if (now - lastCallTime >= wait) {
			func.apply(null, args);
			lastCallTime = now;
		} else {
			lastArgs = args;

			if (!timeout) {
				timeout = setTimeout(
					() => {
						if (lastArgs) {
							func.apply(null, lastArgs);
							lastArgs = null;
						}
						lastCallTime = Date.now();
						timeout = null;
					},
					wait - (now - lastCallTime)
				);
			}
		}
	}

	throttled.cancel = function () {
		if (timeout) {
			clearTimeout(timeout);
			timeout = null;
		}
		lastArgs = null;
		lastCallTime = 0;
	};

	return throttled;
}
