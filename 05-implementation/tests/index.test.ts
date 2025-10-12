/**
 * @fileoverview Basic tests for IEEE Standards Library Entry Point
 * @description Unit tests for main library initialization and version management
 */

import { IEEEStandardsLibrary } from '../src/index';

describe('IEEEStandardsLibrary', () => {
  let library: IEEEStandardsLibrary;

  beforeEach(() => {
    library = new IEEEStandardsLibrary();
  });

  describe('initialization', () => {
    test('should initialize successfully', async () => {
      const result = await library.initialize();
      
      expect(result).toBeDefined();
      expect(result.version).toBeDefined();
      expect(result.standards).toBeDefined();
      expect(Array.isArray(result.standards)).toBe(true);
    });

    test('should return expected version', async () => {
      const result = await library.initialize();
      
      expect(result.version).toMatch(/^\d+\.\d+\.\d+$/); // Semantic version pattern
    });

    test('should return supported IEEE standards', async () => {
      const result = await library.initialize();
      
      expect(result.standards).toContain('IEEE 1588-2019');
      expect(result.standards).toContain('IEEE 1722-2016');
      expect(result.standards).toContain('IEEE 1722.1-2021');
      expect(result.standards).toContain('IEEE 802.1AS-2021');
    });
  });

  describe('version management', () => {
    test('should return version string', () => {
      const version = library.getVersion();
      
      expect(typeof version).toBe('string');
      expect(version).toMatch(/^\d+\.\d+\.\d+$/);
    });

    test('should return consistent version across calls', () => {
      const version1 = library.getVersion();
      const version2 = library.getVersion();
      
      expect(version1).toBe(version2);
    });
  });

  describe('standards support', () => {
    test('should return array of supported standards', () => {
      const standards = library.getSupportedStandards();
      
      expect(Array.isArray(standards)).toBe(true);
      expect(standards.length).toBeGreaterThan(0);
    });

    test('should return immutable copy of standards array', () => {
      const standards1 = library.getSupportedStandards();
      const standards2 = library.getSupportedStandards();
      
      // Should be different array instances (defensive copy)
      expect(standards1).not.toBe(standards2);
      // But should have same content
      expect(standards1).toEqual(standards2);
    });

    test('should include all required IEEE standards', () => {
      const standards = library.getSupportedStandards();
      
      const requiredStandards = [
        'IEEE 1588-2019',
        'IEEE 1722-2016', 
        'IEEE 1722.1-2021',
        'IEEE 802.1AS-2021'
      ];

      requiredStandards.forEach(standard => {
        expect(standards).toContain(standard);
      });
    });
  });
});