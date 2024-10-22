export const formatTime = (seconds: number): string => {
	const h = Math.floor(seconds / 3600);
	const m = Math.floor((seconds % 3600) / 60);

	if (h > 0) {
		return m > 0
			? `${h} hour${h > 1 ? 's' : ''} ${m} min${m > 1 ? 's' : ''}`
			: `${h} hour${h > 1 ? 's' : ''}`;
	}
	return m > 0 ? `${m} min${m > 1 ? 's' : ''}` : `< 1 min`;
};

export const utcToHHMM = (utc: number): string => {
	const date = new Date(utc * 1000); // Convert to milliseconds if it's in seconds
	const hours = date.getUTCHours();
	const minutes = date.getUTCMinutes();
	return `${String(hours).padStart(2, '0')}:${String(minutes).padStart(2, '0')}`;
};

export const formatTimeRange = (startTimestamp: number, endTimestamp: number): string => {
	const startDate = new Date(startTimestamp * 1000);
	const endDate = endTimestamp !== 0 ? new Date(endTimestamp * 1000) : new Date();

	const sameDay = startDate.toDateString() === endDate.toDateString();

	const startFormatted = sameDay
		? `${utcToHHMM(startTimestamp)}`
		: startDate.toLocaleDateString('en-US', {
				month: 'short',
				day: 'numeric',
				timeZone: 'UTC'
			});

	const endFormatted =
		endTimestamp === 0
			? 'now'
			: sameDay
				? utcToHHMM(endTimestamp)
				: endDate.toLocaleString('en-US', {
						month: 'short',
						day: 'numeric',
						timeZone: 'UTC'
					});
	return `${startFormatted} - ${endFormatted}`;
};
