# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ECO304 is a static educational web application for "Introduction to Empirical Economics" at the University of Saskatchewan. It uses **vanilla JavaScript, HTML5, and CSS3** with no build process, no framework, and no package manager.

External dependencies are loaded via CDN only:
- **D3.js v4** — data visualizations
- **Chart.js** — histogram charts in HistoricalGrades
- **jQuery 3.6.4** — DOM interactions in HistoricalGrades
- **Google Analytics** (gtag.js, tracking ID: `G-C0CWMV96MD`) — on every page

## Development

Since there is no build step, development is done by editing files directly and opening them in a browser. The site is deployed as a static site under the `/ECO304/` subdirectory path.

To run locally, serve the repo root with any static file server (e.g., `npx serve .` or `python3 -m http.server`). There are no tests, no linting config, and no CI pipeline.

**Local vs. deployed path handling:** `Scripts/navbar.js` detects `localhost`/`127.0.0.1` and sets the base prefix to `''` locally or `/ECO304` in production. The landing page `index.html` has an inline script that does the same rewrite for feature card `href` attributes.

## Design System

All styles are in `styles.css`. The design language uses:
- **Colors:** dark green `#1a4a28` (primary/headers), `#27ae60` (accent/interactive), `#f0faf4` (light tint backgrounds)
- **Typography:** Inter font family
- **Cards:** white background, `1px solid #e4ede4` border, `border-radius: 12px`, subtle `box-shadow` — used across all inner pages
- **Dark header pattern:** inner-page cards have a dark green `.xxx-card-header` (e.g., `.grade-card-header`, `.hist-card-header`, `.gloss-chapter-header`) with white text
- **Navbar:** sticky flexbox bar with `.nav-wordmark` (ECO304 wordmark), `.nav-links`, and `.nav-hamburger` hamburger toggle for mobile (`@media (max-width: 768px)`)
