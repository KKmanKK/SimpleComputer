.PHONY: all clean run FORCE

# ── Build everything ────────────────────────────────────────────────────────
all:
	@$(MAKE) -C mySimpleComputer
	@$(MAKE) -C myTerm
	@$(MAKE) -C myBigChars
	@$(MAKE) -C myReadKey
	@$(MAKE) -C console

# ── Generate font file, then launch the console ─────────────────────────────
# font.bin is placed in console/ so the console binary finds it by default.
run: all
	@$(MAKE) -C console font-run

# ── Clean all build artefacts in every sub-project ──────────────────────────
clean:
	@$(MAKE) -C mySimpleComputer clean
	@$(MAKE) -C myTerm clean
	@$(MAKE) -C myBigChars clean
	@$(MAKE) -C myReadKey clean
	@$(MAKE) -C console clean
