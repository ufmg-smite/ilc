# Introdução à Lógica Computacional

Course websites for DCC638 (DCC-UFMG), one directory per semester:

- `2026.1/` — https://ufmg-smite.github.io/ilc/2026.1/
- `2026.2/` — https://ufmg-smite.github.io/ilc/2026.2/

Each directory is a self-contained Jekyll site (just-the-docs theme). The root
`index.html` is a landing page linking to the semesters.

Deployment is done by `.github/workflows/pages.yml`, which builds every
semester site and publishes them together to GitHub Pages (the repo's Pages
source must be set to "GitHub Actions").

## Adding a new semester

1. Copy the previous semester's directory (or the scaffolding plus whatever
   material carries over) to `YYYY.N/`.
2. Update `baseurl` in `YYYY.N/_config.yml` to `/ilc/YYYY.N`.
3. Add a build step for it in `.github/workflows/pages.yml`.
4. Add a link in the root `index.html`.

## Local preview

From the repo root (one shared `Gemfile`):

```sh
bundle install
bundle exec jekyll serve -s 2026.2 -d _site/2026.2
```

Then open http://localhost:4000/ilc/2026.2/ (the configured `baseurl` is kept
when serving locally).
