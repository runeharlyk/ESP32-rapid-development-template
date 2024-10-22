import { writable } from 'svelte/store';

type StateType = 'info' | 'success' | 'warning' | 'error' | 'progress';

type State = {
	id: string;
	type: StateType;
	message: string;
	timeoutId?: ReturnType<typeof setTimeout>;
};

function createNotificationStore() {
	const state: State[] = [];
	const notifications = writable(state);
	const { subscribe } = notifications;

	function send(message: string, type: StateType = 'info', timeout: number, id?: string) {
		const toastId = id || generateId();

		notifications.update((state) => {
			let existing = state.find((n) => n.id === toastId);

			if (existing) {
				clearTimeout(existing.timeoutId);
				existing = { ...existing, message, type };
			} else {
				existing = {
					id: toastId,
					type,
					message,
					timeoutId: undefined
				};
				state = [...state, existing];
			}

			existing.timeoutId = setTimeout(
				() => notifications.update((state) => state.filter((n) => n.id !== toastId)),
				timeout
			);

			return state.map((n) => (n.id === toastId ? existing : n));
		});
	}

	return {
		subscribe,
		send,
		error: (msg: string, timeout: number, id?: string) => send(msg, 'error', timeout, id),
		warning: (msg: string, timeout: number, id?: string) => send(msg, 'warning', timeout, id),
		info: (msg: string, timeout: number, id?: string) => send(msg, 'info', timeout, id),
		success: (msg: string, timeout: number, id?: string) => send(msg, 'success', timeout, id)
	};
}

function generateId() {
	return '_' + Math.random().toString(36).substr(2, 9);
}

export const notifications = createNotificationStore();
