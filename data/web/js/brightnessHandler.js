function brightnessHandler() {
  return {
    loading: true,
    brightness: "",
    statusMsg: "",

    async fetchBrightness() {
      this.loading = true;
      try {
        const response = await apiFetch("/api/v1/display/brightness");
        const data = await response.json();
        if (!response.ok || !Number.isInteger(data.brightness)) {
          throw new Error(data.message || "Failed to load brightness");
        }
        this.brightness = data.brightness;
        this.statusMsg = "";
      } catch (error) {
        this.statusMsg = error.message || "Failed to load brightness";
      } finally {
        this.loading = false;
      }
    },

    async saveBrightness() {
      const brightness = this.brightness === "" ? NaN : Number(this.brightness);
      if (!Number.isInteger(brightness) || brightness < 0 || brightness > 100) {
        this.statusMsg = "Enter a whole number between 0 and 100";
        return;
      }
      this.loading = true;
      try {
        const response = await apiFetch("/api/v1/display/brightness", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify({ brightness }),
        });
        const data = await response.json();
        if (!response.ok || data.status !== "ok") {
          throw new Error(data.message || "Failed to save brightness");
        }
        this.brightness = data.brightness;
        this.statusMsg = "Brightness updated";
      } catch (error) {
        this.statusMsg = error.message || "Failed to save brightness";
      } finally {
        this.loading = false;
      }
    },

    init() {
      this.fetchBrightness();
    },
  };
}
