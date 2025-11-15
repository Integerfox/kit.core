const fs = require('fs');
const path = require('path');

const wikiDir = process.argv[2] || '.';
const maxLevel = parseInt(process.argv[3] || '3', 10); // include up to H2/H3 etc (H1 is page title)
const sidebarFile = path.join(wikiDir, '_Sidebar.md');

function slugify(raw) {
  // close approximation of GitHub anchor generation
  return raw
    .trim()
    .toLowerCase()
    .replace(/<\/?[^>]+(>|$)/g, '')      // strip HTML tags
    .replace(/[\u2019']/g, '')           // remove apostrophes
    .replace(/[^a-z0-9 _-]/g, '')        // remove punctuation except space and hyphen/underscore
    .replace(/\s+/g, '-')               // spaces to hyphens
    .replace(/-+/g, '-')
    .replace(/^[-_]+|[-_]+$/g, '');
}

function getTitle(content, filename) {
  const lines = content.split(/\r?\n/);
  for (const line of lines) {
    const m = line.match(/^#\s+(.*)$/);
    if (m) return m[1].trim();
  }
  // fall back to filename without extension
  return path.basename(filename, '.md').replace(/-/g, ' ');
}

function getHeadings(content, levelLimit) {
  const lines = content.split(/\r?\n/);
  const headings = [];
  const headingRegex = /^(#{2,6})\s+(.*)$/; // headings from ## to ######
  for (const line of lines) {
    const m = line.match(headingRegex);
    if (m) {
      const level = m[1].length; // number of # characters
      if (level <= levelLimit) {
        headings.push({ level, text: m[2].trim() });
      }
    }
  }
  return headings;
}

const files = fs.readdirSync(wikiDir)
  .filter(f => f.endsWith('.md'))
  .filter(f => f !== '_Sidebar.md')
  .sort((a,b) => a.localeCompare(b, undefined, {sensitivity: 'base'}));

let out = '# Table of contents\n\n';

for (const file of files) {
  try {
    const fp = path.join(wikiDir, file);
    const content = fs.readFileSync(fp, 'utf8');
    const title = getTitle(content, file);
    const pageName = path.basename(file, '.md'); // link target
    // top-level entry
    out += `- [${title}](${encodeURI(pageName)})\n`;

    const headings = getHeadings(content, Math.min(maxLevel, 6));
    for (const h of headings) {
      // create an indent for H2 -> 2 spaces, H3 -> 4 spaces, etc.
      const indent = '  '.repeat(Math.max(0, h.level - 2));
      const anchor = slugify(h.text);
      out += `${indent}- [${h.text}](${encodeURI(pageName)}#${encodeURI(anchor)})\n`;
    }
  } catch (err) {
    console.error('Failed to read', file, err);
  }
}

fs.writeFileSync(sidebarFile, out, 'utf8');
console.log('Wrote', sidebarFile);