const location = import.meta.env.DEV ? "esp32.local" : window.location.host

export default location;