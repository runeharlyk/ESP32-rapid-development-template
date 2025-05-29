type Subscriber<T> = {
  callback: (msg: T) => void
  interval: number
  lastCall: number
  pending?: T
  timer?: ReturnType<typeof setInterval>
}

export class EventBus {
  private static subscribers = new Map<string, Subscriber<unknown>[]>()

  static subscribe<T>(
    eventType: string,
    callback: (msg: T) => void,
    intervalMs: number = 0
  ): string {
    const id = crypto.randomUUID()
    const subscriber: Subscriber<T> = {
      callback,
      interval: intervalMs,
      lastCall: Date.now()
    }

    if (intervalMs > 0) {
      subscriber.timer = setInterval(() => {
        if (subscriber.pending) {
          subscriber.callback(subscriber.pending)
          subscriber.lastCall = Date.now()
          subscriber.pending = undefined
        }
      }, intervalMs)
    }

    const subs = this.subscribers.get(eventType) || []
    subs.push(subscriber as Subscriber<unknown>)
    this.subscribers.set(eventType, subs)

    return id
  }

  static unsubscribe(eventType: string, id: string): void {
    const subs = this.subscribers.get(eventType)
    if (!subs) return

    const index = subs.findIndex(sub => sub.timer?.toString() === id)
    if (index === -1) return

    const sub = subs[index]
    if (sub.timer) {
      clearInterval(sub.timer)
    }

    subs.splice(index, 1)
    if (subs.length === 0) {
      this.subscribers.delete(eventType)
    }
  }

  static publish<T>(eventType: string, msg: T, excludeId?: string): void {
    const subs = this.subscribers.get(eventType)
    if (!subs) return

    const now = Date.now()

    subs.forEach(sub => {
      if (sub.timer?.toString() === excludeId) return

      const elapsed = now - sub.lastCall
      if (sub.interval === 0 || elapsed >= sub.interval) {
        ;(sub.callback as (msg: T) => void)(msg)
        sub.lastCall = now
      } else {
        ;(sub as Subscriber<T>).pending = msg
      }
    })
  }
}
